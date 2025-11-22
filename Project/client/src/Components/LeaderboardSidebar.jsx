import { useState, useEffect } from 'react'
import {
    Card,
    Typography,
    Stack,
    Avatar,
    Box,
    Divider,
    CircularProgress,
    Chip,
} from '@mui/material'
import EmojiEventsIcon from '@mui/icons-material/EmojiEvents'
import { getLeaderboard } from '../APIManager'

export default function LeaderboardSidebar({ userId }) {
    const [leaderboard, setLeaderboard] = useState([])
    const [loading, setLoading] = useState(true)
    const [userRank, setUserRank] = useState(null)

    useEffect(() => {
        const fetchLeaderboard = async () => {
            setLoading(true)
            try {
                const data = await getLeaderboard()
                if (data && data.leaderboard) {
                    const sorted = data.leaderboard.sort((a, b) => b.xp_points - a.xp_points)
                    setLeaderboard(sorted)

                    // Find current user's rank
                    const userIndex = sorted.findIndex(
                        (user) => user.username === localStorage.getItem('currentUsername')
                    )
                    if (userIndex !== -1) {
                        setUserRank(userIndex + 1)
                    }
                }
            } catch (error) {
                console.error('Error fetching leaderboard:', error)
            } finally {
                setLoading(false)
            }
        }

        fetchLeaderboard()
    }, [])

    const getMedalIcon = (rank) => {
        if (rank === 1) return '🥇'
        if (rank === 2) return '🥈'
        if (rank === 3) return '🥉'
        return rank
    }

    const getRankColor = (rank) => {
        if (rank === 1) return '#FFB81C'
        if (rank === 2) return '#C0C0C0'
        if (rank === 3) return '#CD7F32'
        return '#9E9E9E'
    }

    const topUsers = leaderboard.slice(0, 10)

    return (
        <Card
            elevation={0}
            sx={{
                background: '#1a1f3a',
                backdropFilter: 'none',
                border: '3px solid rgba(255, 255, 255, 0.2)',
                borderRadius: '0px',
                p: 3,
                height: 'fit-content',
                position: 'sticky',
                top: '100px',
                boxShadow: '4px 4px 0px rgba(0, 0, 0, 0.15)',
            }}
        >
            <Stack spacing={2}>
                {/* Header */}
                <Box display="flex" alignItems="center" gap={1}>
                    <EmojiEventsIcon sx={{ color: '#FFB81C', fontSize: '1.5rem' }} />
                    <Typography variant="h6" fontWeight={700}>
                        Leaderboard
                    </Typography>
                </Box>

                {/* User's Rank Badge */}
                {userRank && (
                    <Chip
                        label={`Your Rank: #${userRank}`}
                        variant="filled"
                        sx={{
                            background: 'linear-gradient(135deg, #667eea 0%, #764ba2 100%)',
                            color: 'white',
                            fontWeight: 600,
                        }}
                    />
                )}

                <Divider sx={{ my: 1 }} />

                {/* Leaderboard List */}
                {loading ? (
                    <Box display="flex" justifyContent="center" py={3}>
                        <CircularProgress size={40} />
                    </Box>
                ) : topUsers.length > 0 ? (
                    <Stack spacing={1.5}>
                        {topUsers.map((user, index) => {
                            const rank = index + 1
                            const isCurrentUser =
                                user.username === localStorage.getItem('currentUsername')

                            return (
                                <Box
                                    key={index}
                                    sx={{
                                        display: 'flex',
                                        alignItems: 'center',
                                        gap: 1.5,
                                        p: 1.5,
                                        borderRadius: '12px',
                                        background: isCurrentUser
                                            ? 'rgba(102, 126, 234, 0.2)'
                                            : 'rgba(255, 255, 255, 0.3)',
                                        border: isCurrentUser
                                            ? '2px solid #667eea'
                                            : '1px solid rgba(255, 255, 255, 0.1)',
                                        transition: 'all 0.2s ease',
                                        '&:hover': {
                                            background: isCurrentUser
                                                ? 'rgba(102, 126, 234, 0.3)'
                                                : 'rgba(255, 255, 255, 0.4)',
                                        },
                                    }}
                                >
                                    {/* Rank */}
                                    <Box
                                        sx={{
                                            minWidth: '32px',
                                            height: '32px',
                                            display: 'flex',
                                            alignItems: 'center',
                                            justifyContent: 'center',
                                            fontWeight: 700,
                                            fontSize: '1.1rem',
                                            color: getRankColor(rank),
                                        }}
                                    >
                                        {getMedalIcon(rank)}
                                    </Box>

                                    {/* Avatar */}
                                    <Avatar
                                        sx={{
                                            width: 32,
                                            height: 32,
                                            background: `hsl(${rank * 30}, 70%, 60%)`,
                                            fontSize: '0.85rem',
                                            fontWeight: 'bold',
                                        }}
                                    >
                                        {user.username.charAt(0).toUpperCase()}
                                    </Avatar>

                                    {/* Username and XP */}
                                    <Box flex={1} minWidth={0}>
                                        <Typography
                                            variant="body2"
                                            fontWeight={600}
                                            sx={{
                                                overflow: 'hidden',
                                                textOverflow: 'ellipsis',
                                                whiteSpace: 'nowrap',
                                            }}
                                        >
                                            {user.username}
                                        </Typography>
                                        <Typography variant="caption" color="text.secondary">
                                            {user.xp_points} XP
                                        </Typography>
                                    </Box>
                                </Box>
                            )
                        })}
                    </Stack>
                ) : (
                    <Typography variant="body2" color="text.secondary" textAlign="center" py={3}>
                        No leaderboard data available
                    </Typography>
                )}
            </Stack>
        </Card>
    )
}
